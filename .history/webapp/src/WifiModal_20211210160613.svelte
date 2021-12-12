<script>
  export let open = false;
import Modal from "./Modal.svelte";
import { createEventDispatcher } from 'svelte';
import WiFiList from "./WiFiList.svelte";
import { loading_networks, networks, wifi_ssid} from './store.js';

const dispatch = createEventDispatcher();


async function loadWifiNetworks(url) {
  loading_networks.set(true);
  const response = await fetch(url + '/wifi');
  if (response.status == 202 || response.status == 204) {
    setTimeout(() => loadWifiNetworks(url), 5000);
  } else {
    let data = await response.json();
    data = data.filter((ap) => {
      return ap.ssid !== '';
    });
    networks.set(data);
    loading_networks.set(false);
  }
}

function onShown() {
  dispatch('shown');
  loadWifiNetworks('http://192.168.1.230');
}

function networkSelected($e) {
  wifi_ssid.set($e.network.detail.ssid);
}
</script>

<Modal id="wifi-modal" on:closed={() => dispatch('closed')} on:shown={onShown} open={open} title="WiFi Networks">

  <WiFiList on:network={networkSelected}/>

</Modal>