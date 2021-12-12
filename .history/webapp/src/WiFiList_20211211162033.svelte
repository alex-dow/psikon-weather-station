<script>

export let networks = [];

import { createEventDispatcher, onMount } from 'svelte';

const dispatch = createEventDispatcher();

function selectNetwork(network) {
  dispatch('network', network);
}

</script>

<div class="wifi-list-container">
{#if !isLoading}
  {#if networksList.length == 0}
    <p>No wifi networks available.</p>
  {:else}
    <ul class="wifi-networks-list">
      {#each networks as network}
        <li on:click={() => selectNetwork(network)}>
          { network.ssid }
        </li>
      {/each}
    </ul>
  {/if}
{/if}

</div>

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