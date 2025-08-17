import { NextRequest, NextFetchEvent } from 'next/server';
import { NextResponse } from 'next/server';
import { fetchWithTimeout } from './utils/Utils';
import { notFound } from 'next/navigation';
import consola from 'consola';

let rateLimitMap = new Map<string, { count: number; windowStart: number }>();
let allowed: string[] = [];

setInterval(() => {
  rateLimitMap.clear();
  allowed = [];
}, 1000 * 60 * 60);

export async function middleware(request: NextRequest, event: NextFetchEvent) {
  const pathname = request.nextUrl.pathname;
  const ip =
    request.headers.get('x-real-ip') ||
    request.headers.get('x-forwarded-for') ||
    'unknown';

  // Proxy untuk /growtopia/*
  if (pathname.startsWith('/growtopia/')) {
    const targetUrl = `https://${process.env.IP_SERVER}${pathname}${request.nextUrl.search}`;
    consola.log(targetUrl);

    // Ambil body dan headers dari request asli
    const headers = new Headers(request.headers);

    try {
      const response = await fetch(targetUrl, {
        method: request.method,
        headers,
        body: request.body,
        redirect: 'manual', // Hindari redirect otomatis
      });

      // Kembalikan respons dari server tujuan
      return new NextResponse(response.body, {
        status: response.status,
        statusText: response.statusText,
        headers: response.headers,
      });
    } catch (error) {
      consola.error(`[PROXY ERROR] ${targetUrl}`, error);
      return new NextResponse('Proxy Error', { status: 502 });
    }
  }

  // Skip limiter untuk /cache/* + logger
  if (pathname.startsWith('/cache/')) {
    consola.log(`[CACHE ACCESS] ${new Date().toISOString()} | IP: ${ip} | Path: ${pathname}`);
    return NextResponse.next();
  }

  // Check IP only if not in allowed list
  if (!allowed.includes(ip)) {
    try {
      const response = await fetchWithTimeout(
        `http://localhost:8080/check-ip/${ip}`,
        200
      );
      const data = await response.json();
      if (!data.statusCode || data.status || data.is_vpn || data.is_proxy || data.is_vps) {
        return notFound();
      }
    } catch (error) {
      // Kalau error request, skip atau bisa return notFound
    }
    allowed.push(ip);
  }

  // Rate limiting
  const now = Date.now();
  const windowMs = 60000; // 1 menit
  const maxRequests = 10;

  const client = rateLimitMap.get(ip) || { count: 0, windowStart: now };

  if (now - client.windowStart > windowMs) {
    client.count = 0;
    client.windowStart = now;
  }

  if (client.count >= maxRequests) {
    return new Response('Too many requests', { status: 429 });
  }

  client.count++;
  rateLimitMap.set(ip, client);

  return NextResponse.next();
}

export const config = {
  matcher: '/((?!api|_next/static|_next/image|favicon.ico|cache/).*)',
};
