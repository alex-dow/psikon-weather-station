<script>
import { onDestroy, createEventDispatcher, onMount } from 'svelte';
import { loading_networks, networks, wifi_password, wifi_ssid } from './store.js';
import Loader from './Loader.svelte';
import { getAvailableWifiNetworks } from './api.js';

const dispatch = createEventDispatcher();

let isLoading = true;
let networksList = [];

onMount(async () => {
  networksList = await getAvailableWifiNetworks();
  isLoading = false;
})

function selectNetwork(network) {
  dispatch('network', network);
}

</script>

{#if !isLoading}
  {#if networksList.length == 0}
    <p>No wifi networks available.</p>
  {:else}
    <ul class="wifi-networks-list">
      {#each networksList as network}
      <li on:click={() => selectNetwork(network)}>{ network.ssid }</li>
      {/each}
    </ul>
  {/if}
{:else}
<Loader/>
<p>Loading networks ...</p>
{/if}

<style>

.wifi-networks-list {
  margin: 0;
  padding: 0;
  list-style: none;
}

.wifi-networks-list li {
  margin: 0;
  padding: 10px;
  border-bottom: 1px solid #333;
}

.wifi-networks-list li:hover {
  background-color: #eee;
}

.wifi-networks-list li:active {
  background-color: blue;
}

</style>