<script>
import { onDestroy } from 'svelte';
import { fix_and_outro_and_destroy_block } from 'svelte/internal';

import { loading_networks, networks } from './store.js';

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
    <ul>
      {#each $networks as network}
      <li>{ network.ssid }</li>
      {/each}
    </ul>
  {/if}
{:else}
<p>Loading networks ...</p>
{/if}
