import * as crypto from 'crypto';

/**
 * Generates a secure token based on Unix timestamp and secret.
 * 
 * @param secret - The secret key for HMAC generation
 * @param timestamp - Optional timestamp (defaults to current time)
 * @returns Base64-encoded token string
 * 
 * @example
 * ```ts
 * const secret = 'my-secret-key';
 * const token = generateUnixToken(secret);
 * console.log(token); // e.g., "MTYyNzUwMzQ3NjU0Mw==.a1b2c3d4e5f6..."
 * ```
 */
export function generateUnixToken(secret: string, timestamp?: number): string {
  const ts = timestamp ?? Math.floor(Date.now() / 1000);
  const tsBuffer = Buffer.from(ts.toString());
  const signature = crypto.createHmac('sha256', secret)
    .update(tsBuffer)
    .digest('base64url');
  
  return `${tsBuffer.toString('base64url')}.${signature}`;
}

/**
 * Validates a token against the secret and optional time constraints.
 * 
 * @param token - The token string to validate
 * @param secret - The secret key used for HMAC verification
 * @param options - Validation options
 *   - maxAgeSeconds: Maximum token age in seconds (default: 300)
 *   - timestamp: Optional timestamp to validate against (default: current time)
 * @returns Boolean indicating token validity
 * 
 * @example
 * ```ts
 * const secret = 'my-secret-key';
 * const token = generateUnixToken(secret);
 * 
 * // Validate with default no expiration
 * const isValid = validateUnixToken(token, secret);
 * console.log(isValid); // true
 * 
 * // Validate with custom expiration (10 minutes)
 * const isValidCustom = validateUnixToken(token, secret, { maxAgeSeconds: 600 });
 * console.log(isValidCustom); // true
 * 
 * // Validate with time drift tolerance
 * const isValidDrift = validateUnixToken(token, secret, { 
 *   timestamp: Math.floor(Date.now() / 1000) + 30 
 * });
 * console.log(isValidDrift); // true within 30 seconds tolerance
 * ```
 */
export function validateUnixToken(
  token: string, 
  secret: string, 
  options: {
    maxAgeSeconds?: number;
    timestamp?: number;
  } = {}
): boolean {
  const { maxAgeSeconds = 0, timestamp = Math.floor(Date.now() / 1000) } = options;
  
  // Check format
  const parts = token.split('.');
  if (parts.length !== 2) return false;
  
  const [tsEncoded, signature] = parts;
  
  // Decode timestamp
  let tsBuffer: Buffer;
  try {
    tsBuffer = Buffer.from(tsEncoded, 'base64url');
  } catch {
    return false;
  }
  
  const ts = parseInt(tsBuffer.toString(), 10);
  if (isNaN(ts)) return false;
  
  // Check expiration
  if (Math.abs(timestamp - ts) > maxAgeSeconds && maxAgeSeconds > 0) return false;
  
  // Verify signature
  const expectedSignature = crypto.createHmac('sha256', secret)
    .update(tsBuffer)
    .digest('base64url');
  
  return crypto.timingSafeEqual(
    Buffer.from(signature, 'base64url'),
    Buffer.from(expectedSignature, 'base64url')
  );
}

export async function fetchWithTimeout(url: string, timeout: number = 500): Promise<Response> {
  const controller = new AbortController();
  const timeoutId = setTimeout(() => controller.abort(), timeout);
  
  try {
    const response = await fetch(url, {
      signal: controller.signal
    });
    
    clearTimeout(timeoutId);
    return response;
  } catch (error) {
    clearTimeout(timeoutId);
    throw error;
  }
}