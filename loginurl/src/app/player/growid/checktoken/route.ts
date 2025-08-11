import { NextResponse } from "next/server";

export async function POST(req: Request) {
  const body = await req.text();
  const params = new URLSearchParams(body);
  const refreshToken = params.get("refreshToken");
  console.log(refreshToken);

  return new NextResponse(
    JSON.stringify({
      status: "success",
      message: "Account Validated.",
      token: refreshToken || "",
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