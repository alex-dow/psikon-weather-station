<script>
import WiFiList from './WiFiList.svelte';
import WiFiModal from './WifiModal.svelte';
import { loading_networks, networks, wifi_ssid, wifi_password } from './store.js';
import { saveWifiSettings } from './api';




async function loadWifiSettings(url) {
  const response = await fetch(url + '/wifi-settings');
  let data = await response.json();
  console.log('wifi settings:', data);
}

async function onSubmit(e) {
  e.preventDefault();
  saveWifiSettings(wifi_ssid.value, wifi_password);

}

let showWiFiModal = false;

async function onScanClick() {
  showWiFiModal = true;
}

function onModalClosed() {
  showWiFiModal = false;
}



</script>
<WiFiModal open={showWiFiModal} on:closed={onModalClosed}></WiFiModal>
<form id="wifi-form" on:submit={onSubmit}>
<fieldset>
  <legend>WiFi Settings</legend>
  <div class="form-group">
    <label for="wifi_ssid">SSID</label>
    <input id="wifi_ssid" bind:value={$wifi_ssid}/>
  </div>
  <div class="form-group">
    <label for="wifi_password">Password</label>
    <input id="wifi_password" bind:value={$wifi_password}/>
  </div>
  <button id="scan_wifi" on:click={onScanClick} type="button">Scan</button>
  <input type="checkbox" id="wifi_include_hidden">
</fieldset>
<button type="submit">Save</button>
</form>