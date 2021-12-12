<script>
import { networks } from './store.js';

let _networks;

networks.subscribe(value => _networks = value);
</script>

<ul>
  {#each _networks as network}
  <li>{ network.ssid }</li>
  {/each}
</ul>