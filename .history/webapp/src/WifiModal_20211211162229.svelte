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
  loading = true;
  let data = await getAvailableWifiNetworks();
  data = data.filter((d) => (showHiddenNetworks) ? true : d.ssid != '');
  networks = data;
  loading = false;
}

function onShown() {
  dispatch('shown');
  loadWifiNetworks();
}

function networkSelected(network) {
  dispatch('network', network)
  dispatch('closed');
}
</script>

<Modal id="wifi-modal" on:closed={() => dispatch('closed')} on:shown={onShown} open={open} title="WiFi Networks">

  {#if loading}
    <Loader/>
  {:else}
    <WiFiList on:network={networkSelected} networks={networks}/>
  {/if}

</Modal>