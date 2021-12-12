<script>
export let networks = [];
</script>

<ul>
  {#each network in networks}
  <li>{ networks.ssid }</li>
  {/each}
</ul>