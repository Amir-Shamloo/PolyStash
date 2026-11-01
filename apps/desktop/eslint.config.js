// ESLint 9 flat config. Only direct deps are resolvable under pnpm, so no
// @eslint/js / globals / unified typescript-eslint package — minimal inline set.
import tsPlugin from '@typescript-eslint/eslint-plugin';
import tsParser from '@typescript-eslint/parser';

const browserGlobals = {
  window: 'readonly', document: 'readonly', navigator: 'readonly', console: 'readonly',
  location: 'readonly', history: 'readonly', localStorage: 'readonly', fetch: 'readonly',
  URL: 'readonly', alert: 'readonly', crypto: 'readonly', performance: 'readonly',
  setTimeout: 'readonly', clearTimeout: 'readonly', setInterval: 'readonly', clearInterval: 'readonly',
  requestAnimationFrame: 'readonly', cancelAnimationFrame: 'readonly', getComputedStyle: 'readonly',
  matchMedia: 'readonly', process: 'readonly',
  Event: 'readonly', CustomEvent: 'readonly', KeyboardEvent: 'readonly', ErrorEvent: 'readonly',
  HTMLElement: 'readonly', HTMLInputElement: 'readonly', HTMLTextAreaElement: 'readonly',
  HTMLVideoElement: 'readonly', Image: 'readonly', Audio: 'readonly', Blob: 'readonly',
  FileReader: 'readonly', ResizeObserver: 'readonly', MutationObserver: 'readonly',
  DOMParser: 'readonly', Path2D: 'readonly', OffscreenCanvas: 'readonly',
};

export default [
  {
    ignores: ['dist/**', 'node_modules/**', 'src-tauri/**'],
  },
  {
    files: ['src/**/*.{ts,tsx}', 'vite.config.ts'],
    languageOptions: {
      parser: tsParser,
      ecmaVersion: 'latest',
      sourceType: 'module',
      parserOptions: { ecmaFeatures: { jsx: true } },
      globals: browserGlobals,
    },
    plugins: { '@typescript-eslint': tsPlugin },
    linterOptions: { reportUnusedDisableDirectives: 'error' },
    rules: {
      'no-undef': 'off', // TS's checker owns undefined-global detection
      'no-unused-vars': 'off',
      '@typescript-eslint/no-unused-vars': ['error', { argsIgnorePattern: '^_' }],
      'prefer-const': 'error',
      eqeqeq: ['error', 'smart'],
      'no-fallthrough': 'error',
      'no-async-promise-executor': 'error',
      'no-constant-condition': ['error', { checkLoops: false }],
      '@typescript-eslint/no-explicit-any': 'off',
    },
  },
];
