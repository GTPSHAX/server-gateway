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
      src: '/icons/icon-192x192.png',
      sizes: '192x192',
      type: 'image/png',
    },
    {
      src: '/icons/icon-256x256.png',
      sizes: '256x256',
      type: 'image/png',
    },
    {
      src: '/icons/icon-384x384.png',
      sizes: '384x384',
      type: 'image/png',
    },
    {
      src: '/icons/icon-512x512.png',
      sizes: '512x512',
      type: 'image/png',
    },
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