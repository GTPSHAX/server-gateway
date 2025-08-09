import { cookies } from "next/headers";
import ButtonBody from "@/components/ButtonBody";
import Dialog from "@/components/Dialog";
import { century_gothic_bold } from "@/styles/LocalFonts";
import Link from "next/link";
import { validateUnixToken } from "@/utils/Utils";
import { notFound } from "next/navigation";

const APP_TITLE = process.env.NEXT_PUBLIC_APP_TITLE || "GTPS Gateway";
const APP_DESCRIPTION = process.env.NEXT_PUBLIC_APP_DESCRIPTION || "Growtopia Private Server Gateway that makes it easy for players to explore and access multiple servers with a single login URL. Discover and connect to your favorite servers effortlessly.";
const APP_CONTACT_URL = process.env.NEXT_PUBLIC_APP_CONTACT_URL || "https://osm.growplus.asia/contact";
const APP_SECRET_KEY = process.env.APP_SECRET_KEY || "NoSecretKeyProvided";

export default async function Page({
  searchParams,
}: {
  searchParams: Promise<{ [key: string]: string | string[] | undefined }>
}) {
  const cookiesStore = await cookies();
  const sessionCookie = cookiesStore.get('session')?.value;
  if (!sessionCookie || !validateUnixToken(sessionCookie, APP_SECRET_KEY, { maxAgeSeconds: 10 })) {
    return notFound();
  }

  const merchant_name = decodeURIComponent((await searchParams)?.merchant_name as string) ?? '';

  return (
    <div className='w-screen h-screen flex justify-center items-center scale-y-85 bg-transparent bg-none'>
      <Dialog style='default' className={`p-2 bg-[#226f8de8] ${century_gothic_bold.className}`}>
        <h1 className={`text-2xl`}>{APP_TITLE}</h1>
        <p className='mt-1 text-sm text-[#fce6ba]' style={{lineHeight: '1'}}>
          {APP_DESCRIPTION}
        </p>
        <p className='mt-2 text-sm text-[#fce6ba]'>
          <span className='text-[#ff271d]'>Warning</span>: This merchant {merchant_name != "" ? <span>(<span className="text-white">{merchant_name}</span>)</span> : ''} is not registered in our services, or the API key is invalid. Please contact the merchant owner to resolve this issue.<br/>
          <span className="text-[#9e9e9e]">If you are the merchant owner, please check your API key and ensure it is correctly configured.</span>
        </p>
        
        <div className='mt-4 flex gap-2 items-center text-xl'>
          <Link href="/player/validate/close" rel="noopener noreferrer" prefetch={true}>
            <ButtonBody className="bg-[#FFCF00] hover:bg-[#FFCF00]/50 text-white text-balance">
             Nevermind.
            </ButtonBody>
          </Link>
          <Link href={APP_CONTACT_URL} target="_blank" rel="noopener noreferrer" prefetch={true}>
            <ButtonBody className="bg-[#85B9CE] hover:bg-[#85B9CE]/50 text-white text-balance">
              Contact Us.
            </ButtonBody>
          </Link>
        </div>
      </Dialog>
    </div>
  );
}