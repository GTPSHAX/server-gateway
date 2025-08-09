export const dynamic = 'force-dynamic';

const data = {
  name: process.env.NEXT_PUBLIC_APP_NAME || 'GTPS Gateway',
  short_name: process.env.NEXT_PUBLIC_APP_TITLE || 'GTPS',
  description: process.env.NEXT_PUBLIC_APP_DESCRIPTION || 'Growtopia Private Server Gateway',
  start_url: '/',
  display: 'standalone',
  background_color: '#113F67',
  theme_color: '#34699A',
  icons: [
    {
      src: '/favicon.png',
      sizes: '11280x1280',
      type: 'image/png',
    }
  ],
};

export async function GET() {
  return new Response(JSON.stringify(data, null, 4), {
    headers: {
      'Content-Type': 'application/json',
      'Cache-Control': 'public, max-age=3600, immutable',
    },
  });
}