<script>
import WiFiModal from './WifiModal.svelte';
import { saveWifiSettings, getWifiSettings } from './api';
import { onMount } from 'svelte';
import Loader from './Loader.svelte';
import { useFocus } from "svelte-navigator";

const registerFocus = useFocus();
let showWiFiModal = false;
let loading = false;
let settings = {
  wifi_ssid: '',
  wifi_password: ''
}

async function onSubmit(e) {
  e.preventDefault();
  saveWifiSettings(settings);
}

async function onScanClick() {
  showWiFiModal = true;
}


onMount(async () => {
  loading = true;
  settings = await getWifiSettings();
  loading = false;
});

function selectNetwork(network) {
  settings.wifi_ssid = network.ssid;
}

</script>
<WiFiModal bind:open={showWiFiModal} on:network={selectNetwork}></WiFiModal>
<form id="wifi-form" on:submit={onSubmit}>
<fieldset {registerFocus}>
  <legend>WiFi Settings</legend>
  {#if loading}
    <Loader/>
  {:else}
    <button id="scan_wifi" on:click={onScanClick} type="button">Scan for Networks</button>
    <div class="form-group">
      <label for="wifi_ssid">SSID</label>
      <input id="wifi_ssid" bind:value={settings.wifi_ssid} required/>
    </div>

    <div class="form-group">
      <label for="wifi_password">Password</label>
      <input id="wifi_password" type="password" bind:value={settings.wifi_password}/>
    </div>

  {/if}
</fieldset>
<button type="submit" disabled={loading}>{(loading) ? 'Loading' : 'Save'}</button>
<p>
  Remember, after saving you must restart the weather station.
</p>
</form>
