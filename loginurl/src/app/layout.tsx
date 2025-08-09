import type { Metadata, Viewport } from "next";
import "./globals.css";
import { pixel_font7 } from "@/styles/LocalFonts";

const APP_TITLE = process.env.NEXT_PUBLIC_APP_TITLE || "GTPS Gateway";
const APP_NAME = process.env.NEXT_PUBLIC_APP_NAME || "GTPS Gateway - Connect to Growtopia Private Servers";
const APP_DESCRIPTION = process.env.NEXT_PUBLIC_APP_DESCRIPTION || "A gateway for GTPS servers - Connect, Play, and Manage your Geometry Dash Private Servers";
const APP_URL = process.env.NEXT_PUBLIC_APP_URL || "https://osm.growplus.asia";
const APP_CREATOR = process.env.NEXT_PUBLIC_CREATOR || "@oxygenbro";

export const metadata: Metadata = {
  title: {
    default: APP_NAME,
    template: "%s | " + APP_TITLE
  },
  
  description: APP_DESCRIPTION,
  
  keywords: [
    APP_TITLE,
    APP_NAME,
    APP_DESCRIPTION,
    "GTPS", 
    "Growtopia", 
    "Growtopia Private Server", 
    "Private Server", 
    "GTPS Gateway", 
    "GTPS Login URL", 
    "Growtopia Server Hub",
    "Loginurl",
    "Growtopia Gateway",
    "Growtopia Server List",
    "Growtopia Community",
    "Growtopia Server Connection",
    "Growtopia Server Management",
    "Growtopia Server Discovery",
    "Growtopia Server Access",
    "Growtopia Server Gateway",
    "Growtopia Server Login",
    "Loginurl Gratis",
    "Loginurl Gratis Growtopia",
    "Growtopia Server Login URL",
    "loginurl murah"
  ],
  
  metadataBase: new URL(APP_URL),
  alternates: {
    canonical: APP_URL,
  },
  
  openGraph: {
    title: APP_NAME,
    description: APP_DESCRIPTION,
    url: APP_URL,
    siteName: APP_TITLE,
    locale: "en_US",
    type: "website",
  },
  
  twitter: {
    title: APP_NAME,
    description: APP_DESCRIPTION,
    creator: APP_CREATOR,
  },
  
  applicationName: APP_TITLE,
  appleWebApp: {
    capable: true,
    statusBarStyle: "default",
    title: APP_NAME,
  },
  
  robots: {
    index: true,
    follow: true,
    googleBot: {
      index: true,
      follow: true,
      "max-video-preview": -1,
      "max-image-preview": "large",
      "max-snippet": -1,
    },
  },
  
  verification: {
    google: "",
    yandex: "",
  },
  
  icons: {
    icon: "/favicon.ico",
    shortcut: "/favicon.png",
    apple: "/favicon.png",
  },
  
  manifest: "/manifest.json",
};


export const viewport: Viewport = {
  width: "device-width",
  initialScale: 1,
  maximumScale: 1,
  userScalable: false
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en" suppressHydrationWarning>
      <head>
        {/* <link rel="preconnect" href="https://fonts.googleapis.com" />
        <link rel="preconnect" href="https://fonts.gstatic.com" crossOrigin="anonymous" />
        
        <link rel="preload" href="/fonts/pixel-font7.ttf" as="font" type="font/ttf" crossOrigin="anonymous" /> */}
        <link rel="shortcut icon" href="/favicon.ico" type="image/x-icon" />
      </head>
      <body
        className={`antialiased min-h-screen text-foreground ${pixel_font7.className}`}
      >
        {children}
        {/* <script src="https://kit.fontawesome.com/0271dc4eb6.js" crossOrigin="anonymous"></script> */}
      </body>
    </html>
  );
}