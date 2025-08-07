// utils/LocalStorageCache.ts
export class LocalStorageCache {
  private static readonly DEFAULT_MAX_AGE = 3600000; // 1 jam dalam milidetik
  private static readonly DEFAULT_NAMESPACE = 'app_cache';

  /**
   * Simpan data ke localStorage dengan key yang ditentukan
   */
  static setItem<T>(key: string, data: T, maxAge?: number, namespace?: string): void {
    try {
      const cacheKey = `${namespace || this.DEFAULT_NAMESPACE}:${key}`;
      const item = {
        data,
        timestamp: Date.now(),
        maxAge: maxAge || this.DEFAULT_MAX_AGE,
        version: '1.0'
      };
      
      const serializedData = JSON.stringify(item);
      localStorage.setItem(cacheKey, serializedData);
    } catch (error) {
      console.error(`Gagal menyimpan ${key} ke localStorage:`, error);
    }
  }

  /**
   * Ambil data dari localStorage berdasarkan key
   */
  static getItem<T>(key: string, namespace?: string): T | null {
    try {
      const cacheKey = `${namespace || this.DEFAULT_NAMESPACE}:${key}`;
      const serializedData = localStorage.getItem(cacheKey);
      
      if (serializedData === null) return null;
      
      const item = JSON.parse(serializedData);
      
      // Cek apakah cache masih valid
      if (this.isExpired(item)) {
        this.removeItem(key, namespace);
        return null;
      }
      
      return item.data;
    } catch (error) {
      console.error(`Gagal membaca ${key} dari localStorage:`, error);
      return null;
    }
  }

  /**
   * Hapus item dari localStorage
   */
  static removeItem(key: string, namespace?: string): void {
    try {
      const cacheKey = `${namespace || this.DEFAULT_NAMESPACE}:${key}`;
      localStorage.removeItem(cacheKey);
    } catch (error) {
      console.error(`Gagal menghapus ${key} dari localStorage:`, error);
    }
  }

  /**
   * Hapus semua item cache
   */
  static clearAll(namespace?: string): void {
    try {
      const prefix = namespace ? `${namespace}:` : '';
      const keysToRemove: string[] = [];
      
      for (let i = 0; i < localStorage.length; i++) {
        const key = localStorage.key(i);
        if (key && key.startsWith(prefix)) {
          keysToRemove.push(key);
        }
      }
      
      keysToRemove.forEach(key => localStorage.removeItem(key));
    } catch (error) {
      console.error('Gagal membersihkan cache:', error);
    }
  }

  /**
   * Periksa apakah cache sudah kadaluarsa
   */
  static isExpired(item: any): boolean {
    if (!item) return true;
    if (item.maxAge === 0) return false; // Tidak ada batas waktu
    if (!item.timestamp) return true;
    return Date.now() - item.timestamp > item.maxAge;
  }

  /**
   * Perbarui cache dengan data terbaru
   */
  static updateItem<T>(key: string, data: T, maxAge?: number, namespace?: string): void {
    this.setItem(key, data, maxAge, namespace);
  }

  /**
   * Cek apakah key ada di cache
   */
  static hasItem(key: string, namespace?: string): boolean {
    try {
      const cacheKey = `${namespace || this.DEFAULT_NAMESPACE}:${key}`;
      const item = localStorage.getItem(cacheKey);
      return item !== null;
    } catch (error) {
      return false;
    }
  }

  /**
   * Dapatkan informasi cache
   */
  static getCacheInfo(key: string, namespace?: string): any {
    try {
      const cacheKey = `${namespace || this.DEFAULT_NAMESPACE}:${key}`;
      const serializedData = localStorage.getItem(cacheKey);
      
      if (serializedData === null) return null;
      
      const item = JSON.parse(serializedData);
      return {
        key: cacheKey,
        timestamp: item.timestamp,
        isExpired: this.isExpired(item),
        age: Date.now() - item.timestamp,
        maxAge: item.maxAge
      };
    } catch (error) {
      return null;
    }
  }

  /**
   * Ambil semua key yang dimiliki oleh namespace tertentu
   */
  static getAllKeys(namespace?: string): string[] {
    try {
      const prefix = namespace ? `${namespace}:` : '';
      const keys: string[] = [];
      
      for (let i = 0; i < localStorage.length; i++) {
        const key = localStorage.key(i);
        if (key && key.startsWith(prefix)) {
          keys.push(key.replace(prefix, ''));
        }
      }
      
      return keys;
    } catch (error) {
      console.error('Gagal mengambil semua key:', error);
      return [];
    }
  }

  /**
   * Cache data dari API dengan fallback ke cache
   */
  static async cacheApiData<T>(
    url: string, 
    key: string, 
    maxAge?: number, 
    namespace?: string,
    options?: RequestInit
  ): Promise<T | null> {
    try {
      // Coba ambil dari cache dulu
      const cachedData = this.getItem<T>(key, namespace);
      if (cachedData !== null) {
        return cachedData;
      }

      // Jika tidak ada cache, fetch dari API
      const response = await fetch(url, options);
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      
      const data = await response.json();
      
      // Simpan ke cache
      this.setItem(key, data, maxAge, namespace);
      
      return data;
    } catch (error) {
      console.error(`Gagal cache API ${url}:`, error);
      return null;
    }
  }

  /**
   * Cache semua konten dengan key yang disediakan
   */
  static async cacheAllContent<T>(
    contentMap: Record<string, { url: string; maxAge?: number }>,
    namespace?: string
  ): Promise<Record<string, T | null>> {
    const results: Record<string, T | null> = {};
    
    for (const [key, config] of Object.entries(contentMap)) {
      results[key] = await this.cacheApiData<T>(
        config.url, 
        key, 
        config.maxAge, 
        namespace
      );
    }
    
    return results;
  }
}

// Export interface untuk type safety
export interface CacheItem<T> {
  data: T;
  timestamp: number;
  maxAge: number;
  version: string;
}

export interface CacheInfo {
  key: string;
  timestamp: number;
  isExpired: boolean;
  age: number;
  maxAge: number;
}