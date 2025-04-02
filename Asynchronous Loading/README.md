# Asynchronous Loading

To increase speed the scene loader and model loader were remade to work with threads.

## ModelLoader

Threading **FBX SDK** seemed impossible for that reason we assigned one thread to the modelLoader for loading meshes and animations. An AssetManager would request for an asset from the ModelLoader which the request would be added to an queue and return an id for the request.

While running, the ModelLoader will pop requests to continue working.

At the end of frame the assetManager will check the modelLoader if the request has been processed. 


## SceneLoader

Entities (such as Components) need to communicate with each other as well as register to the Entity system which meant that there would be two cache misses every time an entity was created slowing the program down.

To increase the speed of this process, the loader will run a thread for every type of entity and the process is divided into five Phases.

### Phase 0: Read file.

The threads have a shared stream between them to read a file. Reading base data for components and extra data that is stored.

### Phase 1: Set extracted data.

Create an amount of entities that are separated from the system and set values that were stored in phase 0.

### Phase 2: Communicate with other objects Ex set Objects Hierarchy

Components need data about hierarchy for what state the component is in. The work around was to store the transformers state that represents the hierarchy in a buffer that it will retrieve.

### Phase 3: Finalize

Append all entities to their managers and store their pointers inside the loading buffer for easier access for other components.

### Phase 4: Awake

This will awaken all entities.
