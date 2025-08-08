// import { Suspense } from 'react';
// import GrowtopiaLogin from './GrowtopiaLogin';

import Dialog from "@/app/components/Dialog";
import { century_gothic_bold } from "@/styles/LocalFonts";

const APP_TITLE = process.env.NEXT_PUBLIC_APP_TITLE || "GTPS Gateway";
const APP_DESCRIPTION = process.env.NEXT_PUBLIC_APP_DESCRIPTION || "Growtopia Private Server Gateway that makes it easy for players to explore and access multiple servers with a single login URL. Discover and connect to your favorite servers effortlessly.";

export default function ({ 
  searchParams 
}: { 
  searchParams: { [key: string]: string | string[] | undefined } 
}) {
  const merchant_name = searchParams.merchant_name || '';

  return (
    <div className='w-screen h-screen flex justify-center items-center scale-y-85 bg-transparent bg-none'>
      <Dialog style='default' className={`p-2 bg-[#226f8de8] ${century_gothic_bold.className}`}>
        <h1 className={`text-xl`}>{APP_TITLE}</h1>
        <p className='mt-1 text-sm text-[#fce6ba]' style={{lineHeight: '1'}}>
          {APP_DESCRIPTION}
        </p>
        <p className='mt-2 text-sm text-[#fce6ba]'>
          <span className='text-[#ff271d]'>Warning:</span> This merchant {merchant_name != "" ? <span>(<span className="text-white">{merchant_name}</span>)</span> : ''} is not registered in our services, or the API key is invalid. Please contact the merchant owner to resolve this issue.
        </p>
      </Dialog>
    </div>
    
    // <Suspense fallback={<div className='w-screen h-screen flex justify-center items-center'>Loading...</div>}>
    //   <GrowtopiaLogin />
    // </Suspense>
  );
}