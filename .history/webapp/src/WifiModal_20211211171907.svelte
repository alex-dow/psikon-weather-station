<script>
export let open = false;
import Modal from "./Modal.svelte";
import { createEventDispatcher } from 'svelte';
import WiFiList from "./WiFiList.svelte";
import { getAvailableWifiNetworks } from './api.js';
import Loader from "./Loader.svelte";

const dispatch = createEventDispatcher();

let showHiddenNetworks = false;
let loading = false;
let networks = [];

async function loadWifiNetworks() {
  console.log('loadWifiNEtworks()');
  loading = true;
  let data = await getAvailableWifiNetworks();
  data = data.filter((d) => (showHiddenNetworks) ? true : d.ssid != '');
  networks = data;
  loading = false;
}

function onShown() {
  console.log('onshown');

  loadWifiNetworks();
}

function networkSelected(network) {
  dispatch('network', network)
  open = false;
}
</script>

<Modal id="wifi-modal" on:shown={onShown} bind:open={open} title="WiFi Networks">

  {#if loading}
    <Loader/>
  {:else}
    <WiFiList on:network={networkSelected} networks={networks}/>
  {/if}

</Modal>