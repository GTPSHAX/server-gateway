import type { FastifyInstance, FastifyPluginOptions } from 'fastify';
import { Reader } from '@maxmind/geoip2-node'
import fs from 'fs';
import path from 'path';
import OpenAI from 'openai';
import { randomInt } from 'crypto';
import { match } from 'assert';

const OPENROUTER_API_KEYS = ["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", "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", "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", "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", "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", "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", "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", "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", "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"]

const route = async (fastify: FastifyInstance, options: FastifyPluginOptions) => {
  fastify.get('/:ip', async (request, reply) => {
    const { ip } = request.params as { ip: string } || "";

    if (!ip || ip === null || ip === "")
      return reply.status(400).send({ status: "error", message: "/:ip Is required!" });

      const ASN = await Reader.open(path.join(__dirname, "../../data/GeoLite2-ASN.mmdb"));
      const City = await Reader.open(path.join(__dirname, "../../data/GeoLite2-City.mmdb"));
      const Country = await Reader.open(path.join(__dirname, "../../data/GeoLite2-Country.mmdb"));

      const asn = ASN.asn(ip);
      const asn_str = JSON.stringify(asn);
      const city = City.city(ip);
      const city_str = JSON.stringify(city);
      const country = Country.country(ip);
      const country_str = JSON.stringify(country);

      const bannedFilePath = path.join(__dirname, "../../data/banned.txt");
      const bannedText = fs.readFileSync(bannedFilePath, "utf-8");
      const banned = bannedText.split("\n").map(line => line.trim()).filter(line => line.length > 0);

      const ctx = (`${asn_str} ${city_str} ${country_str}`).toUpperCase();
      console.log(ctx);

      const isBanned = banned.some(txt => {
        if (txt.length > 0) {
          const regex = new RegExp(txt, 'i');
          return regex.test(ctx);
        }
        return false;
      });

      if (isBanned) {
        return {
          status: true
        };
      }

    console.log(asn, city, country);

    const client = new OpenAI({
      baseURL: "https://openrouter.ai/api/v1",
      apiKey: OPENROUTER_API_KEYS[randomInt(OPENROUTER_API_KEYS.length)]
    });

    const dataset = `Im will only reply JSON stringfy like this "{ is_vpn: true, is_vps: true, is_proxy: true, presentence: 80 }"\nIm as security checker for validating IP and return the results as JSON stringfy, this are banned list: ${bannedText}`
    const question = ctx;

    const completion = await client.chat.completions.create({
      model: 'openai/gpt-oss-20b:free',
      messages: [
        {
          role: 'system',
          content: dataset
        },
        {
          role: 'user',
          content: question
        },
      ],
    });

    const messageContent = completion?.choices[0]?.message?.content;
    if (typeof messageContent !== 'string') {
      return {
        status: false
      };
    }
    return JSON.parse(messageContent);
  });
};

export default route;