import { cookies } from "next/headers";
import ButtonBody from "@/components/ButtonBody";
import Dialog from "@/components/Dialog";
import { century_gothic_bold } from "@/styles/LocalFonts";
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
  // if (!sessionCookie || validateUnixToken(sessionCookie, APP_SECRET_KEY) === false) {
  //   return notFound();
  // }

  const merchant_name = (await searchParams)?.merchant_name ?? '';
  const data = (await searchParams)?.data ?? "";

  return (
    <div className='w-screen h-screen flex justify-center items-center scale-y-85 bg-transparent bg-none'>
      <Dialog style='default' className={`p-2 bg-[#226f8de8] ${century_gothic_bold.className}`}>
        <h1 className={`text-2xl`}>{merchant_name}</h1>
        <p className='mt-1 text-sm text-[#fce6ba]' style={{lineHeight: '1'}}>
          {APP_DESCRIPTION}
        </p>
        
        <form className='mt-3' method="POST"
        action="/player/growid/login/validate"
        acceptCharset="UTF-8"
        id="loginForm">
          <input type="text" name="_token" defaultValue={data} hidden />
          <input type="text" name="session" defaultValue={sessionCookie} hidden />
          <input type="text" name="merchant_namw" defaultValue={merchant_name} hidden />

          <div className="flex flex-col gap-2">
            <label className='text-sm text-[#fce6ba]'>
              GrowID:
              <Dialog style="default" className="w-full">
                <input
                  type="text"
                  className="text-sm p-1 bg-[#85B9CE]/30 w-full active:outline-none focus:outline-none text-white rounded-none"
                  placeholder="Enter your GrowID"
                />
              </Dialog>
            </label>
            <label className='text-sm text-[#fce6ba]'>
              GrowID:
              <Dialog style="default" className="w-full">
                <input
                  type="text"
                  className="text-sm p-1 bg-[#85B9CE]/30 w-full active:outline-none focus:outline-none text-white rounded-none"
                  placeholder="Enter your GrowID"
                />
              </Dialog>
            </label>
          </div>
        </form>

        <div className='mt-4 flex gap-2 items-center text-xl w-full'>
          <button className="flex-1">
            <ButtonBody className="bg-[#FFCF00] w-full hover:bg-[#FFCF00]/50 text-white text-balance">
              Guest
            </ButtonBody>
          </button>
          <button className="flex-1">
            <ButtonBody className="bg-[#85B9CE] w-full hover:bg-[#85B9CE]/50 text-white text-balance">
              Login
            </ButtonBody>
          </button>
        </div>
      </Dialog>
    </div>
  );
}