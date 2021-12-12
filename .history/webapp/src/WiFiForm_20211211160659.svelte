<script>
import WiFiModal from './WifiModal.svelte';
import { wifi_ssid, wifi_password } from './store.js';
import { saveWifiSettings, getWifiSettings } from './api';
import { onMount } from 'svelte';
import Loader from './Loader.svelte';
import { useFocus } from "svelte-navigator";

const registerFocus = useFocus();
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
<fieldset {registerFocus}>
  <legend>WiFi Settings</legend>
  {#if loading}
    <Loader/>
  {:else}
    <button id="scan_wifi" on:click={onScanClick} type="button">Scan for Networks</button>
    <div class="form-group">
      <label for="wifi_ssid">SSID</label>
      <input id="wifi_ssid" bind:value={$wifi_ssid} required/>
    </div>

    <div class="form-group">
      <label for="wifi_password">Password</label>
      <input id="wifi_password" type="password" bind:value={$wifi_password}/>
    </div>

  {/if}
</fieldset>
<button type="submit" disabled={loading}>{(loading) ? 'Loading' : 'Save'}</button>
<p>
  Note: after saving you must restart the weather station.
</p>
</form>

<style>

.overlay {
  position: relative;
  top: 0;
  left: 0;
  background-color: #000000;
  opacity: 0.3;
  z-index: 9999;
  display: hide;
}

</style>