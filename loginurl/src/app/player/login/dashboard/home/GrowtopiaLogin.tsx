'use client';
import { useState, useEffect, useRef } from 'react';
import { useSearchParams } from 'next/navigation';

const GrowtopiaLogin: React.FC = () => {
  const searchParams = useSearchParams();
  const [growId, setGrowId] = useState('');
  const [password, setPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);
  const [clicked, setClicked] = useState(false);
  const formRef = useRef<HTMLFormElement>(null);

  // Get data from URL parameters
  const serverName = searchParams.get('server_name') || "Growtopia";
  const token = searchParams.get('data') || "";

  useEffect(() => {
    // Set document title and meta tags
    document.title = 'Growtopia Player Support';
    
    // Set favicon
    const setFavicon = (href: string) => {
      let link = document.querySelector("link[rel~='icon']") as HTMLLinkElement;
      if (!link) {
        link = document.createElement('link');
        link.rel = 'icon';
        document.getElementsByTagName('head')[0].appendChild(link);
      }
      link.href = href;
    };
    
    setFavicon('https://s3.eu-west-1.amazonaws.com/cdn.growtopiagame.com/website/resources/assets/images/growtopia.ico');
    
    // Set shortcut icon
    let shortcutIcon = document.querySelector("link[rel='shortcut icon']") as HTMLLinkElement;
    if (!shortcutIcon) {
      shortcutIcon = document.createElement('link');
      shortcutIcon.rel = 'shortcut icon';
      shortcutIcon.type = 'image/x-icon';
      document.getElementsByTagName('head')[0].appendChild(shortcutIcon);
    }
    shortcutIcon.href = 'https://s3.eu-west-1.amazonaws.com/cdn.growtopiagame.com/website/resources/assets/images/growtopia.ico';

    // Load saved GrowID from localStorage on component mount
    const savedGrowId = localStorage.getItem('growId');
    if (savedGrowId) {
      setGrowId(savedGrowId);
    }

    // Keyboard event listeners for dev tools prevention
    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.key === 'F12' || 
          e.keyCode === 123 ||
          (e.ctrlKey && e.shiftKey && e.key === 'I') ||
          (e.ctrlKey && e.key === 'I') ||
          (e.ctrlKey && e.shiftKey && e.key === 'C') ||
          (e.ctrlKey && e.shiftKey && e.key === 'J') ||
          (e.ctrlKey && e.key === 'U')) {
        e.preventDefault();
      }
    };

    document.addEventListener('keydown', handleKeyDown);

    // Mobile scaling observer
    const observer = new MutationObserver((mutations) => {
      mutations.forEach((mutation) => {
        mutation.addedNodes.forEach((node) => {
          if (node.nodeType === Node.ELEMENT_NODE && (node as Element).tagName === 'DIV') {
            const element = node as HTMLElement;
            const screenWidth = window.screen.width;
            if (screenWidth < 667) {
              element.style.transform = 'scale(0.75)';
              element.style.transformOrigin = '0 0';
              element.style.webkitTransform = 'scale(0.75)';
              element.style.webkitTransformOrigin = '0 0';
              element.style.overflow = 'auto';
            }
          }
        });
      });
    });

    observer.observe(document.body, {
      attributes: false,
      childList: true,
      characterData: false
    });

    // Handle anchor click prevention (replacing jQuery functionality)
    const handleAnchorClick = (e: Event) => {
      const target = e.target as HTMLAnchorElement;
      if (clicked === false) {
        setClicked(true);
        return true;
      }
      target.setAttribute('onclick', 'return false;');
      e.preventDefault();
    };

    const anchors = document.querySelectorAll('a');
    anchors.forEach(anchor => {
      anchor.addEventListener('click', handleAnchorClick);
    });

    // Cleanup
    return () => {
      document.removeEventListener('keydown', handleKeyDown);
      observer.disconnect();
      anchors.forEach(anchor => {
        anchor.removeEventListener('click', handleAnchorClick);
      });
    };
  }, [clicked]);

  const handleLogin = () => {
    if (!growId.trim()) {
      return;
    }
    
    // Save GrowID to localStorage
    localStorage.setItem('growId', growId);
    
    // Submit form
    if (formRef.current) {
      formRef.current.submit();
    }
  };

  const handleGuest = () => {
    setGrowId('');
    setPassword('');
    localStorage.setItem('growId', '');
    
    if (formRef.current) {
      formRef.current.submit();
    }
  };

  const togglePasswordVisibility = () => {
    setShowPassword(!showPassword);
  };

  return (
    <div className="card">
      <div className="logo">
        <h1>{serverName} Dashboard</h1>
      </div>
      
      <p style={{ color: '#BBE1FA' }}>
        Please enter your <b style={{ color: '#FFC045' }}>GrowID</b> and{' '}
        <b style={{ color: '#FFC045' }}>Password</b> to log in to your existing account, or log in as a guest.
      </p>

      <form
        ref={formRef}
        method="POST"
        action="/player/growid/login/validate"
        acceptCharset="UTF-8"
        id="loginForm"
        className="mt-3"
        onSubmit={handleLogin}
      >
        <input name="_token" value={token} type="hidden" />
        <input name="nameServer" value={serverName} type="hidden" />
        
        <div className="form">
          <div className="growid">
            <label htmlFor="loginGrowId"><strong>GrowID</strong></label>
            <div className="input">
              <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="currentColor" className="bi bi-person-fill" viewBox="0 0 16 16">
                <path d="M3 14s-1 0-1-1 1-4 6-4 6 3 6 4-1 1-1 1zm5-6a3 3 0 1 0 0-6 3 3 0 0 0 0 6"/>
              </svg>
              <input
                type="text"
                name="growId"
                id="loginGrowId"
                placeholder="Enter your growid"
                value={growId}
                onChange={(e) => setGrowId(e.target.value)}
              />
            </div>
          </div>

          <div className="password">
            <label htmlFor="loginPassword"><strong>Password</strong></label>
            <div className="input">
              <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="currentColor" className="bi bi-key-fill" viewBox="0 0 16 16">
                <path d="M3.5 11.5a3.5 3.5 0 1 1 3.163-5H14L15.5 8 14 9.5l-1-1-1 1-1-1-1 1-1-1-1 1H6.663a3.5 3.5 0 0 1-3.163 2M2.5 9a1 1 0 1 0 0-2 1 1 0 0 0 0 2"/>
              </svg>
              <input
                type={showPassword ? "text" : "password"}
                name="password"
                id="loginPassword"
                placeholder="Enter your password"
                value={password}
                onChange={(e) => setPassword(e.target.value)}
              />
              <button
                type="button"
                id="toggleLogPassword"
                onClick={togglePasswordVisibility}
                style={{ marginRight: '10px' }}
              >
                {showPassword ? (
                  <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="currentColor" className="bi bi-eye-slash-fill" viewBox="0 0 16 16">
                    <path d="m10.79 12.912-1.614-1.615a3.5 3.5 0 0 1-4.474-4.474l-2.06-2.06C.938 6.278 0 8 0 8s3 5.5 8 5.5a7 7 0 0 0 2.79-.588M5.21 3.088A7 7 0 0 1 8 2.5c5 0 8 5.5 8 5.5s-.939 1.721-2.641 3.238l-2.062-2.062a3.5 3.5 0 0 0-4.474-4.474z"/>
                    <path d="M5.525 7.646a2.5 2.5 0 0 0 2.829 2.829zm4.95.708-2.829-2.83a2.5 2.5 0 0 1 2.829 2.829zm3.171 6-12-12 .708-.708 12 12z"/>
                  </svg>
                ) : (
                  <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="currentColor" className="bi bi-eye-fill" viewBox="0 0 16 16">
                    <path d="M10.5 8a2.5 2.5 0 1 1-5 0 2.5 2.5 0 0 1 5 0"/>
                    <path d="M0 8s3-5.5 8-5.5S16 8 16 8s-3 5.5-8 5.5S0 8 0 8m8 3.5a3.5 3.5 0 1 0 0-7 3.5 3.5 0 0 0 0 7"/>
                  </svg>
                )}
              </button>
            </div>
          </div>

          <div className="button">
            <button id="guestButton" type="button" onClick={handleGuest}>
              Guest
            </button>
            <button id="loginButton" type="submit">
              Login
            </button>
          </div>
        </div>
      </form>
    </div>
  );
};

export default GrowtopiaLogin;