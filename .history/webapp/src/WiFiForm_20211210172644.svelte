<script>
import WiFiList from './WiFiList.svelte';
import WiFiModal from './WifiModal.svelte';
import { loading_networks, networks, wifi_ssid, wifi_password } from './store.js';




async function loadWifiSettings(url) {
  const response = await fetch(url + '/wifi-settings');
  let data = await response.json();
  console.log('wifi settings:', data);
}

async function saveWifiSettings(url) {

  const formData = new FormData();
  formData.append('wifi_ssid', wifi_ssid);
  formData.append('wifi_password', wifi_password);

  const response = await fetch(url + '/wifi-settings', {
    method: 'POST',
    body: formData
  });
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
<form id="wifi-form" action="http://192.168.1.203/wifi-settings" method="POST">
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