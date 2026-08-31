-- migrate:up
CREATE TABLE tokens (
    id BIGSERIAL PRIMARY KEY,
    owner_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    token_hash TEXT NOT NULL UNIQUE,
    device_uuid UUID NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    expires_at TIMESTAMPTZ NOT NULL CHECK (expires_at > created_at),

    CONSTRAINT tokens_one_per_device UNIQUE (owner_id, device_uuid)
);

-- migrate:down
DROP TABLE tokens;
