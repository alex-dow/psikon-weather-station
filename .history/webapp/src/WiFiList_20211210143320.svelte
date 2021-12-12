<script>
import { loading_networks, networks } from './store.js';

let isLoading = false;
let networksList = [];
const unsub_loading = loading_networks.subscribe(value => isLoading = value);
const unsub_networks = networks.subscribe(value => networksList = value);

</script>

{#if $loading_networks === false}
<ul>
  {#each $networks as network}
  <li>{ network.ssid }</li>
  {/each}
</ul>
{:else}
<p>Loading networks ...</p>
{/if}
