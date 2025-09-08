import React, { useRef, useEffect, useState } from 'react';
import './Shuffle.css';

interface ShuffleProps {
  text: string;
  shuffleDirection?: 'left' | 'right';
  duration?: number;
  animationMode?: 'sequential' | 'random' | 'evenodd';
  shuffleTimes?: number;
  ease?: string;
  stagger?: number;
  threshold?: number;
  triggerOnce?: boolean;
  triggerOnHover?: boolean;
  respectReducedMotion?: boolean;
  className?: string;
  style?: React.CSSProperties;
}

const Shuffle: React.FC<ShuffleProps> = ({
  text,
  shuffleDirection = 'right',
  duration = 0.35,
  animationMode = 'evenodd',
  shuffleTimes = 1,
  ease = 'ease-out',
  stagger = 0.03,
  threshold = 0.1,
  triggerOnce = true,
  triggerOnHover = true,
  respectReducedMotion = true,
  className = '',
  style = {}
}) => {
  const [isVisible, setIsVisible] = useState(false);
  const [isAnimating, setIsAnimating] = useState(false);
  const [shuffledText, setShuffledText] = useState(text);
  const [hasAnimated, setHasAnimated] = useState(false);
  const containerRef = useRef<HTMLDivElement>(null);
  const observerRef = useRef<IntersectionObserver | null>(null);
  const animationTimeoutRef = useRef<NodeJS.Timeout | null>(null);

  // Check for reduced motion preference
  const prefersReducedMotion = respectReducedMotion &&
    window.matchMedia &&
    window.matchMedia('(prefers-reduced-motion: reduce)').matches;

  useEffect(() => {
    if (prefersReducedMotion) {
      setIsVisible(true);
      return;
    }

    const observer = new IntersectionObserver(
      (entries) => {
        entries.forEach((entry) => {
          if (entry.isIntersecting) {
            setIsVisible(true);
            if (triggerOnce && observerRef.current) {
              observerRef.current.disconnect();
            }
          }
        });
      },
      { threshold }
    );

    if (containerRef.current) {
      observer.observe(containerRef.current);
      observerRef.current = observer;
    }

    return () => {
      if (observerRef.current) {
        observerRef.current.disconnect();
      }
      if (animationTimeoutRef.current) {
        clearTimeout(animationTimeoutRef.current);
        animationTimeoutRef.current = null;
      }
    };
  }, [threshold, triggerOnce, prefersReducedMotion]);

  // Cleanup on unmount
  useEffect(() => {
    return () => {
      if (animationTimeoutRef.current) {
        clearTimeout(animationTimeoutRef.current);
        animationTimeoutRef.current = null;
      }
    };
  }, [threshold, triggerOnce, prefersReducedMotion]);

  const shuffleText = async () => {
    if (isAnimating || prefersReducedMotion) return;

    // Clear any existing timeout
    if (animationTimeoutRef.current) {
      clearTimeout(animationTimeoutRef.current);
      animationTimeoutRef.current = null;
    }

    setIsAnimating(true);

    const chars = text.split('');
    let currentShuffled = [...chars];

    // Initialize with random characters for non-space positions
    for (let i = 0; i < chars.length; i++) {
      if (chars[i] !== ' ') {
        currentShuffled[i] = String.fromCharCode(65 + Math.floor(Math.random() * 26));
      }
    }

    setShuffledText(currentShuffled.join(''));

    // Shuffle animation cycles
    for (let cycle = 0; cycle < shuffleTimes; cycle++) {
      for (let i = 0; i < chars.length; i++) {
        if (chars[i] !== ' ') {
          currentShuffled[i] = String.fromCharCode(65 + Math.floor(Math.random() * 26));
          setShuffledText(currentShuffled.join(''));

          await new Promise<void>(resolve => {
            animationTimeoutRef.current = setTimeout(() => resolve(), (duration * 1000) / chars.length);
          });
        }
      }
    }

    // Reveal original text based on animation mode
    if (animationMode === 'sequential') {
      for (let i = 0; i < chars.length; i++) {
        currentShuffled[i] = chars[i];
        setShuffledText(currentShuffled.join(''));

        await new Promise<void>(resolve => {
          animationTimeoutRef.current = setTimeout(() => resolve(), stagger * 1000);
        });
      }
    } else if (animationMode === 'evenodd') {
      // First reveal even indices
      for (let i = 0; i < chars.length; i += 2) {
        currentShuffled[i] = chars[i];
      }
      setShuffledText(currentShuffled.join(''));

      await new Promise<void>(resolve => {
        animationTimeoutRef.current = setTimeout(() => resolve(), stagger * 1000);
      });

      // Then reveal odd indices
      for (let i = 1; i < chars.length; i += 2) {
        currentShuffled[i] = chars[i];
      }
      setShuffledText(currentShuffled.join(''));
    } else if (animationMode === 'random') {
      const remainingIndices = chars
        .map((char, index) => ({ char, index }))
        .filter(({ char }) => char !== ' ')
        .map(({ index }) => index);

      // Shuffle the indices array
      for (let i = remainingIndices.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [remainingIndices[i], remainingIndices[j]] = [remainingIndices[j], remainingIndices[i]];
      }

      for (const index of remainingIndices) {
        currentShuffled[index] = chars[index];
        setShuffledText(currentShuffled.join(''));

        await new Promise<void>(resolve => {
          animationTimeoutRef.current = setTimeout(() => resolve(), stagger * 1000);
        });
      }
    }

    setIsAnimating(false);
  };

  useEffect(() => {
    if (isVisible && !hasAnimated && !isAnimating) {
      shuffleText().catch(error => {
        console.error('Shuffle animation error:', error);
        setIsAnimating(false);
        setShuffledText(text);
      });
      setHasAnimated(true);
    }
  }, [isVisible, hasAnimated, isAnimating, text, shuffleTimes, animationMode, duration, stagger]);

  const handleHover = () => {
    if (triggerOnHover && !isAnimating && hasAnimated) {
      shuffleText();
    }
  };

  return (
    <div
      ref={containerRef}
      className={`shuffle-container ${className} ${isVisible ? 'visible' : ''}`}
      style={style}
      onMouseEnter={handleHover}
    >
      <span className="shuffle-text">
        {shuffledText}
      </span>
    </div>
  );
};

export default Shuffle;