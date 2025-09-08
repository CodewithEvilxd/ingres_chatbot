import React, { useState } from 'react';
import './FloatingActionButton.css';

interface FloatingActionButtonProps {
  onClick: () => void;
  icon: string;
  label: string;
  color?: string;
  size?: 'small' | 'medium' | 'large';
}

const FloatingActionButton: React.FC<FloatingActionButtonProps> = ({
  onClick,
  icon,
  label,
  color = '#2563eb',
  size = 'medium'
}) => {
  const [isHovered, setIsHovered] = useState(false);

  const sizeClasses = {
    small: 'fab-small',
    medium: 'fab-medium',
    large: 'fab-large'
  };

  return (
    <div className="fab-container">
      <button
        className={`fab ${sizeClasses[size]}`}
        onClick={onClick}
        onMouseEnter={() => setIsHovered(true)}
        onMouseLeave={() => setIsHovered(false)}
        style={{
          '--fab-color': color,
          '--fab-shadow': `0 8px 25px ${color}40`,
          '--fab-glow': `0 0 20px ${color}60`
        } as React.CSSProperties}
        aria-label={label}
      >
        <span className="fab-icon">{icon}</span>
        <div className="fab-ripple"></div>
      </button>

      {isHovered && (
        <div className="fab-tooltip">
          {label}
        </div>
      )}
    </div>
  );
};

export default FloatingActionButton;