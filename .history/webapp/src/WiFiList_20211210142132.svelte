<script>
import { loading_networks, networks } from './store.js';
import WiFiList from './WiFiList.svelte';

let _networks;
let _loading_networks;

networks.subscribe(value => _networks = value);
loading_networks.subscribe(value => _loading_networks = value);
</script>

{#if _loading_networks === false}
<ul>
  {#each _networks as network}
  <li>{ network.ssid }</li>
  {/each}
</ul>
{:else}
<p>Loading networks ...</p>
{/if}

<WiFiList/>