<script>
import { loading_networks, networks } from './store.js';

let _networks;
let _loading_networks;

networks.subscribe(value => _networks = value);
loading_networks.subscribe(value => _loading_networks = value);
</script>

<ul>
  {#each _networks as network}
  <li>{ network.ssid }</li>
  {/each}
</ul>