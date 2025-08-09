import consola from "consola";
import { existsSync } from "fs";
import { generateUnixToken } from "@/utils/Utils";
import { notFound } from "next/navigation";
import path from "path";
import fs from "fs";

const APP_SECRET_KEY = process.env.APP_SECRET_KEY || "NoSecretKeyProvided";

// Redirect 404 page
export async function GET() {
  return notFound();
}

export async function POST(req: Request) {
  const tData: Record<string, string> = {};
  let status = 403;
  let merchant_name = "GTPS";

  try {
    const body = await req.text();
    const decoded = decodeURIComponent(body);
    const lines = decoded.split('\n');

    for (const line of lines) {
      if (!line.includes('|')) continue;
      const [key, value] = line.split('|');
      tData[key] = value;
    }

    // API Key validation
    if (tData["meta"] !== "") {
      const params = new URLSearchParams(tData["meta"]);
      const merchant = params.get("merchant");
      merchant_name = merchant as string || '';
      const apiKey = params.get("key");

      if (merchant || apiKey) {
        const file_path = path.join(process.cwd(), "..", "database", "merchants", merchant + ".json");
        consola.log(file_path);
        if (!existsSync(file_path)) {
          throw new Error(`Merchant file not found: ${file_path}`);
        }
        
        const data = JSON.parse(fs.readFileSync(file_path, "utf-8"));
        if (merchant !== data.name && apiKey !== data.api_key) {
          throw new Error(`Invalid merchant or API key: ${merchant}, ${apiKey}`);
        }
        merchant_name = data.name;
        status = 200;
      }
    }
  } catch (err) {
    consola.error("Error:", err);
  }

  return new Response(null,
    status != 200 ? {
      status: 302,
      headers: {
        'Location': `dashboard/denied?merchant_name=${encodeURIComponent(merchant_name)}`,
        'Set-Cookie': `session=${generateUnixToken(APP_SECRET_KEY)}; Path=/player/; HttpOnly; Secure; SameSite=Lax`
      }
    } : {
      status: 302,
      headers: {
        'Location': `dashboard/home?data=${encodeURIComponent(JSON.stringify(tData))}&merchant_name=${encodeURIComponent(merchant_name)}`,
        'Set-Cookie': `session=${generateUnixToken(APP_SECRET_KEY)}; Path=/player/; HttpOnly; Secure; SameSite=Lax`
      }
    }
  );
}