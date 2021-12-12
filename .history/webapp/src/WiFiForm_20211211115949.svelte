<script>
import WiFiList from './WiFiList.svelte';
import WiFiModal from './WifiModal.svelte';
import { loading_networks, networks, wifi_ssid, wifi_password } from './store.js';
import { saveWifiSettings, getWifiSettings } from './api';
import { onMount } from 'svelte';

let showWiFiModal = false;
let loading = false;

async function onSubmit(e) {
  e.preventDefault();
  saveWifiSettings($wifi_ssid, $wifi_password);
}

async function onScanClick() {
  showWiFiModal = true;
}

function onModalClosed() {
  showWiFiModal = false;
}

onMount(async () => {
  loading = true;
  const data = await getWifiSettings();
  wifi_ssid.set(data.wifi_ssid);
  loading = false;
});

</script>
<WiFiModal open={showWiFiModal} on:closed={onModalClosed}></WiFiModal>
<form id="wifi-form" on:submit={onSubmit}>
<fieldset>
  <div class="overload"></div>
  <legend>WiFi Settings</legend>
  <div class="form-group">
    <label for="wifi_ssid">SSID</label>
    <input id="wifi_ssid" bind:value={$wifi_ssid} :disabled={loading}/>
  </div>
  <div class="form-group">
    <label for="wifi_password">Password</label>
    <input id="wifi_password" bind:value={$wifi_password} :disabled={loading}/>
  </div>
  <button id="scan_wifi" on:click={onScanClick} type="button">Scan</button>
  <input type="checkbox" id="wifi_include_hidden">
</fieldset>
<button type="submit" disabled={loading}>{(loading) ? 'Loading' : 'Save'}</button>
</form>