<script>
import { onMount, createEventDispatcher, afterUpdate } from "svelte";

export let title = '';
export let open = false;
export let id = '';

const dispatch = createEventDispatcher();

function hideModal() {
  const el = document.getElementById(id);
  if (!el) {
    console.warn('Modal for id ' + id + ' does not exist');
  } else {
    el.style.display = 'none';
  }
  dispatch('closed', {});
}

function showModal() {
  const el = document.getElementById(id);
  if (!el) {
    console.warn('Modal for id ' + id + ' does not exist');
  } else {
    el.style.display = 'block';
  }
  dispatch('shown', {});
}

function closeButton() {
  open = false;
}


afterUpdate(() => {
  (open) ? showModal() : hideModal();
});

</script>
<div class="modal" id={id}>
  <div class="modal-body">
    <div class="modal-header">
      <slot name="modal-header">
        <h1>{title}</h1>
      </slot>
    </div>
    <div class="modal-content">
      <slot></slot>
    </div>
    <div class="modal-footer">
      <slot name="modal-footer">
        <button type="button" id={id + '-close-button'} on:click={closeButton}>Close</button>
      </slot>
    </div>
  </div>

</div>

<style>

.modal {
  display: none; /* Hidden by default */
  position: fixed; /* Stay in place */
  z-index: 1; /* Sit on top */
  left: 0;
  top: 0;
  width: 100%; /* Full width */
  height: 100%; /* Full height */
  overflow: auto; /* Enable scroll if needed */
  background-color: rgb(0,0,0); /* Fallback color */
  background-color: rgba(0,0,0,0.4); /* Black w/ opacity */
}

/* Modal Content/Box */
.modal-body {
  background-color: #fefefe;
  margin: 15% auto; /* 15% from the top and centered */
  padding: 20px;
  border: 1px solid #888;
  max-width: 400px;
  width: 90%;
}

.modal-close-button {
  color: #aaaaaa;
  float: right;
  font-size: 28px;
  font-weight: bold;
}

.modal-header {
  text-align: center;
}

.modal-footer {
  text-align: center;
}

.modal-content {
  clear: both;
  text-align: center;
}


</style>