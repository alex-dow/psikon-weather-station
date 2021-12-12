<script>
import WiFiList from './WiFiList.svelte';
import { loading_networks } from './store.js';

async function onClick() {
  console.log('!?');
  loading_networks.set(true);
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