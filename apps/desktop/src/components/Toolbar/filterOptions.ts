export interface DateFilterOption {
  label: string;
  from: number;
}

export interface SizeFilterOption {
  label: string;
  min: number;
  max: number;
}

export interface PreviewFilterOption {
  status: 'done' | 'pending' | 'none';
  label: string;
}

export const getStartOfDay = (): number => {
  const d = new Date();
  d.setHours(0, 0, 0, 0);
  return d.getTime();
};

export const getDateOptions = (now = Date.now()): DateFilterOption[] => [
  { label: 'Today', from: getStartOfDay() },
  { label: 'Last 7 Days', from: now - 7 * 86400 * 1000 },
  { label: 'Last 30 Days', from: now - 30 * 86400 * 1000 },
];

export const SIZE_OPTIONS: SizeFilterOption[] = [
  { label: '< 5 MB', min: 0, max: 5 * 1024 * 1024 },
  { label: '5 – 50 MB', min: 5 * 1024 * 1024, max: 50 * 1024 * 1024 },
  { label: '> 50 MB', min: 50 * 1024 * 1024, max: 10 * 1024 * 1024 * 1024 },
];

export const PREVIEW_OPTIONS: PreviewFilterOption[] = [
  { status: 'done', label: 'Has Preview' },
  { status: 'pending', label: 'Pending / Generating' },
  { status: 'none', label: 'Needs Preview' },
];
