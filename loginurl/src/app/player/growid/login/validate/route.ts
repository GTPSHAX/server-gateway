import { NextResponse } from 'next/server';
import { URLSearchParams } from 'url';
import { cookies } from 'next/headers';
import { validateUnixToken, generateUnixToken } from '@/utils/Utils';

const APP_SECRET_KEY = process.env.APP_SECRET_KEY || "NoSecretKeyProvided";
const APP_NAME = process.env.NEXT_PUBLIC_APP_NAME || "GTPS Gateway";

export async function POST(req: Request) {
  const cookie = await cookies();
  const session = cookie.get('session')?.value;
  if (!session || !validateUnixToken(session, APP_SECRET_KEY, { maxAgeSeconds: 60 * 5 })) {
    return new NextResponse(
      JSON.stringify({
        status: "failed",
        message: `\`o${APP_NAME}: \`4Something went wrong... Bad connection or internet? No one knows.`,
        token: "",
        url: "",
        accountType: "growtopia",
      }),
      {
        status: 200,
        headers: {
          "Content-Type": "text/html",
        },
      }
    );
  }

  const account_session = generateUnixToken(APP_SECRET_KEY);

  const rawBody = await req.text();
  const params = new URLSearchParams(rawBody);

  const _token: string = params.get('_token') || "";
  const growId: string = params.get('growId') || "";
  const password: string = params.get('password') || "";
  const merchant_name: string = params.get('merchant_name') || "";

  const token = Buffer.from(
    `_token=${_token}&_session=${account_session}&merchant_name=${merchant_name}&growId=${growId}&password=${password}`
  ).toString('base64');

  // Mengirim respons JSON
  return new NextResponse(
    JSON.stringify({
      status: "success",
      message: "Account Validated.",
      token: token || "",
      url: "",
      accountType: "growtopia",
    }),
    {
      status: 200,
      headers: {
        "Content-Type": "text/html",
      },
    }
  );
}