import React from 'react';

interface ButtonProps {
  children: React.ReactNode;
  className?: string;
}

const ButtonBody: React.FC<ButtonProps> = ({ className = '', children }) => {
  return (
    <div
      className={`px-1 py-2 rounded-[2px] transition-all duration-300 text-sh ${className}` }
      style={{
        boxShadow: "1px 1px 0 rgba(0,0,0,0.5)",
      }}
    >
      {children}
    </div>
  );
}

export default ButtonBody;