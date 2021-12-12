<script>
import { loading_networks, networks } from './store.js';



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
