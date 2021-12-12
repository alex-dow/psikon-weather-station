import { writable } from 'svelte/store';

export const networks = writable([]);
export const loading_networks = writable(false);

export const wifi_ssid = writable('');
export const wifi_password = writable('');

export const mqtt_host = writable('');
export const mqtt_port = writable(1883);
export const mqtt_username = writable('');
export const mqtt_password = writable('');
export const mqtt_id = writable('psikon-weather-station-' + Math.floor(Math.random() * 99999));

export const restarted = writable(false);