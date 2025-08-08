// import { Suspense } from 'react';
// import GrowtopiaLogin from './GrowtopiaLogin';

import Card from "@/app/components/Dialog";

export default function HomePage() {
  return (
    <div className='w-screen h-screen flex justify-center items-center'>
      <Card style='default' className='w-96 h-96 flex justify-center items-center'>
        <h1 className='text-2xl font-bold'>Welcome to the Dashboard</h1>
      </Card>
    </div>
    
    // <Suspense fallback={<div className='w-screen h-screen flex justify-center items-center'>Loading...</div>}>
    //   <GrowtopiaLogin />
    // </Suspense>
  );
}