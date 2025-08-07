import { MetadataRoute } from 'next';

const APP_URL = process.env.NEXT_PUBLIC_APP_URL || "https://osm.growplus.asia";

export default function robots(): MetadataRoute.Robots {
  return {
    rules: {
      userAgent: '*',
      allow: '/',
      disallow: ['/private/', '/admin/', '/player/login/'],
    },
    sitemap: APP_URL + '/sitemap.xml',
  };
}