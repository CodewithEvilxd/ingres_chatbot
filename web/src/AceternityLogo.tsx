import React from 'react';

const AceternityLogo: React.FC<{ className?: string }> = ({ className = '' }) => {
  return (
    <svg
      width="24"
      height="24"
      viewBox="0 0 66 65"
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
      className={`text-current ${className}`}
    >
      <path
        d="M8 8.05571C8 8.05571 54.9009 18.1782 57.8687 30.062C60.8365 41.9458 9.05432 57.4696 9.05432 57.4696"
        stroke="currentColor"
        strokeWidth="3"
        strokeMiterlimit="3.86874"
        strokeLinecap="round"
        fill="none"
      />
    </svg>
  );
};

export default AceternityLogo;