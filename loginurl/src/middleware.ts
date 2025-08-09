import { NextRequest, NextFetchEvent } from 'next/server';
import { NextResponse } from 'next/server';
import { fetchWithTimeout } from './utils/Utils';
import { notFound } from 'next/navigation';

let rateLimitMap = new Map<string, { count: number; windowStart: number }>();
let allowed: string[] = [];

setInterval(()=>{
  rateLimitMap.clear();
  allowed = [];
}, 1000 * 60 * 60);

export async function middleware(request: any, event: NextFetchEvent) {
  const ip = request.ip || request.headers.get('x-real-ip') || request.headers.get('x-forwarded-for');

  // Check ip
  if (!allowed.includes(ip)) {
    try {
      const response = await fetchWithTimeout(`http://localhost:8080/check-ip/${ip}`, 200);
      const data = await response.json();
      console.log(data);
      if (!data.statusCode || data.status || data.is_vpn || data.is_proxy || data.is_vps) {
        return notFound();
      }
    } catch (error) {
    }
  }
  allowed.push(ip);

  const now = Date.now();
  const windowMs = 60000;
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
  matcher: '/((?!api|_next/static|_next/image|favicon.ico).*)',
};