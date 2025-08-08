import consola from "consola";
import { existsSync } from "fs";

export async function POST(req: Request) {
  const tData: Record<string, string> = {};
  let status = 403;
  let merchant_name = "Growtopia";

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
      const apiKey = params.get("key");

      if (merchant || apiKey) {
        const file_path = `./src/database/merchant/${merchant}.json`;
        if (!existsSync(file_path)) {
          throw new Error(`Merchant file not found: ${file_path}`);
        }
        
        const data = await import(file_path);
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
    status === 403 ? {
      status: 302,
      headers: {
        'Location': `dashboard/denied?merchant_name=${merchant_name}`
      }
    } : {
      status: 302,
      headers: {
        'Location': `dashboard/home?data=${encodeURIComponent(JSON.stringify(tData))}&merchant_name=${merchant_name}`
      }
    }
  );
}