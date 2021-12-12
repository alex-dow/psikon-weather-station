<script>
export let networks = [];
</script>

<ul>
  {#each networks as network}
  <li>{ network.ssid }</li>
  {/each}
</ul>