-- migrate:up
CREATE TYPE friendship_status AS ENUM ('pending', 'accepted');

CREATE TABLE friendships (
    user_1 BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    user_2 BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    status friendship_status NOT NULL DEFAULT 'pending',
    initiated_by BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    PRIMARY KEY (user_1, user_2),

    CONSTRAINT friendships_no_self_friend CHECK (user_1 != user_2),
    CONSTRAINT friendships_ordered CHECK (user_1 < user_2),
    CONSTRAINT friendships_initiator_valid CHECK (
        initiated_by = user_1 OR initiated_by = user_2 
    )
);

CREATE TABLE blocks (
    blocker_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    blocked_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),

    PRIMARY KEY(blocker_id, blocked_id),
    CONSTRAINT blocks_no_self_block CHECK (blocker_id != blocked_id)
);

-- migrate:down
DROP TABLE friendships;
DROP TABLE blocks;
DROP TYPE friendship_status;
