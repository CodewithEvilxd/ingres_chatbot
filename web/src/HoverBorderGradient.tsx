import React, { useState, useEffect } from 'react';
import './HoverBorderGradient.css';

interface HoverBorderGradientProps {
  children: React.ReactNode;
  containerClassName?: string;
  className?: string;
  as?: React.ElementType;
  duration?: number;
  clockwise?: boolean;
  onClick?: () => void;
}

const HoverBorderGradient: React.FC<HoverBorderGradientProps> = ({
  children,
  containerClassName = '',
  className = '',
  as: Tag = 'button',
  duration = 2,
  clockwise = true,
  onClick
}) => {
  const [hovered, setHovered] = useState(false);
  const [direction, setDirection] = useState<'top' | 'left' | 'bottom' | 'right'>('top');

  const directions: Array<'top' | 'left' | 'bottom' | 'right'> = ['top', 'left', 'bottom', 'right'];

  const rotateDirection = (currentDirection: typeof direction): typeof direction => {
    const currentIndex = directions.indexOf(currentDirection);
    const nextIndex = clockwise
      ? (currentIndex - 1 + directions.length) % directions.length
      : (currentIndex + 1) % directions.length;
    return directions[nextIndex];
  };

  useEffect(() => {
    if (!hovered) {
      const interval = setInterval(() => {
        setDirection(prevState => rotateDirection(prevState));
      }, duration * 1000);
      return () => clearInterval(interval);
    }
  }, [hovered, duration, clockwise]);

  return (
    <Tag
      className={`hover-border-gradient ${containerClassName}`}
      onMouseEnter={() => setHovered(true)}
      onMouseLeave={() => setHovered(false)}
      onClick={onClick}
      style={{
        '--duration': `${duration}s`,
        '--direction': direction
      } as React.CSSProperties}
    >
      <div className={`hover-border-gradient-content ${className}`}>
        {children}
      </div>
      <div className="hover-border-gradient-border"></div>
      <div className="hover-border-gradient-glow"></div>
    </Tag>
  );
};

export default HoverBorderGradient;