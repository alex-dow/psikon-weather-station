<script>
import WiFiList from './WiFiList.svelte';
import { loading_networks } from './store.js';


async function loadWifiNetworks(url) {
  const response = await fetch(url + '/wifi');
  if (response.status == 202 || response.status == 204) {
    setTimeout(() => loadWifiNetowrks(url), 5000);
  } else {
    loading_networks.set(false);
  }
}

async function onClick() {
  loading_networks.set(true);
  loadWifiNetworks('http://192.168.1.230');
}

</script>
<WiFiList/>
<fieldset>
  <legend>WiFi Settings</legend>
  <div class="form-group">
    <label for="wifi_ssid">SSID</label>
    <input id="wifi_ssid"/>
  </div>
  <div class="form-group">
    <label for="wifi_password">Password</label>
    <input id="wifi_password"/>
  </div>
  <button id="scan_wifi" on:click={onClick} type="button">Scan</button>
</fieldset>