import { writable } from 'svelte/store';

export const networks = writable([]);
export const loading_networks = writable(false);

export const wifi_ssid = writable('');
export const wifi_password = writable('');