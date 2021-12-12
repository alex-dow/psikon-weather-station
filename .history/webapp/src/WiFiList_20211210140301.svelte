<script>
export let networks = [];
</script>

<ul>
  {#each networks as network}
  <li>{ networks.ssid }</li>
  {/each}
</ul>