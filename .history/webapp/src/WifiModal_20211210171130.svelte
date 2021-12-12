<script>
  export let open = false;
import Modal from "./Modal.svelte";
import { createEventDispatcher } from 'svelte';
import WiFiList from "./WiFiList.svelte";
import { loading_networks, networks, wifi_ssid} from './store.js';
import { getAvailableWifiNetworks } from './api.js';

const dispatch = createEventDispatcher();

let showHiddenNetworks = false;

async function loadWifiNetworks(url) {
  loading_networks.set(true);
  let data = await getAvailableWifiNetworks();
  data = data.filter((d) => (showHiddenNetworks) ? true : d.ssid != '');

  networks.set(data);
  loading_networks.set(false);
}

function onShown() {
  dispatch('shown');
  loadWifiNetworks('http://192.168.1.230');
}

function networkSelected($e) {
  wifi_ssid.set($e.detail.ssid);
  dispatch('closed');
}
</script>

<Modal id="wifi-modal" on:closed={() => dispatch('closed')} on:shown={onShown} open={open} title="WiFi Networks">

  <WiFiList on:network={networkSelected}/>

</Modal>