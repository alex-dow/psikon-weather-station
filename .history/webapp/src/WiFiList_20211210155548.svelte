<script>
import { onDestroy } from 'svelte';
import { loading_networks, networks } from './store.js';
import Loader from './Loader.svelte';

let isLoading = false;
let networksList = [];
const unsub_loading = loading_networks.subscribe(value => {
  console.log('loading changed');
  isLoading = value
});
const unsub_networks = networks.subscribe(value => networksList = value);

onDestroy(() => {
  unsub_loading();
  unsub_networks();
});

</script>

{#if !isLoading}
  {#if networksList.length == 0}
    <p>No wifi networks available.</p>
  {:else}
    <ul class="wifi-networks-list">
      {#each $networks as network}
      <li>{ network.ssid }</li>
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
  padding: 0;
}

</style>