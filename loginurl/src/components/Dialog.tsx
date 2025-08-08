import React from 'react';

interface DialogProps {
  style?: string;
  children: React.ReactNode;
  className?: string;
}

const Card: React.FC<DialogProps> = ({ 
  style = 'default', 
  children, 
  className = '' 
}) => {
  const getCardClasses = () => {
    // const baseClasses = "p-4 rounded-lg shadow-md transition-all duration-300";
    
    // switch (style) {
    //   case 'yes':
    //     return baseClasses + "border-2 border-green-500 bg-green-50 ";
    //   case 'no':
    //     return baseClasses + "border-2 border-red-500 bg-red-50 ";
    //   default:
    //     return baseClasses + "border border-gray-200 bg-white ";
    // }
  };

  return (
    <div className={`border-2 border-[#88B9CA] rounded-[2px] max-h-screen overflow-y-auto ${getCardClasses()} ${className}`}
      style={{
        boxShadow: "1px 1px 0 rgba(0,0,0,0.5), inset 1px 1px 0 rgba(0,0,0,0.5)"
      }}>
      {children}
    </div>
  );
};

export default Card;